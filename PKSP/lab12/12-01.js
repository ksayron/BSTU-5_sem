// server.js
const http = require('http');
const fs = require('fs');
const url = require('url');
const path = require('path');
const WebSocket = require('ws');

let x = require('./StudentList.json')

const STUDENT_LIST = 'StudentList.json';
const BACKUP_DIR = 'backups';
const PORT = 3000;

function loadStudents() {
  try {
    if (!fs.existsSync(STUDENT_LIST)) return [];
    const data = fs.readFileSync(STUDENT_LIST, 'utf8');
    if (!data) return [];
    return JSON.parse(data);
  } catch (e) {
    return { error: 1, message: 'ошибка чтения файла StudentList.json' };
  }
}

function saveStudents(students) {
  fs.writeFileSync(STUDENT_LIST, JSON.stringify(students, null, 4), 'utf8');
}

function backupStudents() {
  if (!fs.existsSync(BACKUP_DIR)) fs.mkdirSync(BACKUP_DIR);
  const timestamp = new Date().toISOString().replace(/[-:.TZ]/g, "").slice(0, 8);
  const backupFile = path.join(BACKUP_DIR, `${timestamp}_StudentList.json`);
  fs.copyFileSync(STUDENT_LIST, backupFile);
  return backupFile;
}

let wss;
function notifyAll(payload) {
  if (!wss) return;
  const msg = JSON.stringify(payload);
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(msg);
    }
  });
}

if (!fs.existsSync(BACKUP_DIR)) {
  try { fs.mkdirSync(BACKUP_DIR); } catch(e){ console.error(e); }
}

const server = http.createServer((req, res) => {
  const parsed = url.parse(req.url, true);
  const pathname = parsed.pathname;
  const method = req.method.toUpperCase();

  if (method === 'GET' && pathname === '/') {
    const students = loadStudents();
    if (students.error) {
      res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(students));
      return;
    }
    res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
    res.end(JSON.stringify(students));
    return;
  }

  const idMatch = pathname.match(/^\/(\d+)$/);
  if (method === 'GET' && idMatch) {
    const id = parseInt(idMatch[1]);
    const students = loadStudents();
    if (students.error) {
      res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(students));
      return;
    }
    const student = students.find(s => s.id === id);
    if (!student) {
      res.writeHead(404, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify({ error: 2, message: `студент с id ${id} не найден` }));
      return;
    }
    res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
    res.end(JSON.stringify(student));
    return;
  }

  if (method === 'POST' && pathname === '/') {
    let body = '';
    req.on('data', chunk => { body += chunk.toString(); });
    req.on('end', () => {
      if (!body) {
        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 1, message: 'Тело запроса не должно быть пустым' }));
        return;
      }
      let newStudent;
      try {
        newStudent = JSON.parse(body);
      } catch (e) {
        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 2, message: 'Недопустимый JSON' }));
        return;
      }
      if (newStudent.id === undefined || newStudent.name === undefined) {
        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 3, message: 'Недостаточно данных: id и name обязательны' }));
        return;
      }
      const students = loadStudents();
      if (students.error) {
        res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify(students));
        return;
      }
      if (students.some(s => s.id === newStudent.id)) {
        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 4, message: `Студент с id ${newStudent.id} уже существует` }));
        return;
      }
      students.push(newStudent);
      saveStudents(students);
      notifyAll({ event: 'students-updated', action: 'add', student: newStudent });
      res.writeHead(201, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(newStudent));
    });
    return;
  }

  if (method === 'PUT' && pathname === '/') {
    let body = '';
    req.on('data', chunk => { body += chunk.toString(); });
    req.on('end', () => {
      if (!body) {
        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 1, message: 'Тело запроса не должно быть пустым' }));
        return;
      }
      let updated;
      try { updated = JSON.parse(body); } catch (e) {
        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 2, message: 'Недопустимый JSON' }));
        return;
      }
      if (updated.id === undefined) {
        res.writeHead(400, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 3, message: 'id обязателен для обновления' }));
        return;
      }
      const students = loadStudents();
      if (students.error) {
        res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify(students));
        return;
      }
      const idx = students.findIndex(s => s.id === updated.id);
      if (idx === -1) {
        res.writeHead(404, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 2, message: `студент с id ${updated.id} не найден` }));
        return;
      }
      students[idx] = updated;
      saveStudents(students);
      notifyAll({ event: 'students-updated', action: 'update', student: updated });
      res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(updated));
    });
    return;
  }

  if (method === 'DELETE' && idMatch) {
    const id = parseInt(idMatch[1]);
    const students = loadStudents();
    if (students.error) {
      res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(students));
      return;
    }
    const idx = students.findIndex(s => s.id === id);
    if (idx === -1) {
      res.writeHead(404, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify({ error: 2, message: `студент с id ${id} не найден` }));
      return;
    }
    const deleted = students.splice(idx, 1)[0];
    saveStudents(students);
    notifyAll({ event: 'students-updated', action: 'delete', student: deleted });
    res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
    res.end(JSON.stringify(deleted));
    return;
  }

  if (method === 'POST' && pathname === '/backup') {
    setTimeout(() => {
      try {
        const backupFile = backupStudents();
        notifyAll({ event: 'backup', action: 'created', file: backupFile });
        res.writeHead(201, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ message: 'Backup created', backup_file: backupFile }));
      } catch (e) {
        res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 1, message: 'Ошибка создания бэкапа' }));
      }
    }, 2000);
    return;
  }

  const delBackupMatch = pathname.match(/^\/backup\/(\d{8})$/);
  if (method === 'DELETE' && delBackupMatch) {
    const dateStr = delBackupMatch[1];
    const year = parseInt(dateStr.slice(0, 4));
    const month = parseInt(dateStr.slice(4, 6)) - 1; 
    const day = parseInt(dateStr.slice(6, 8));
    const cutoff = new Date(year, month, day);

    fs.readdir(BACKUP_DIR, (err, files) => {
      if (err) {
        res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 1, message: 'ошибка чтения директории backups' }));
        return;
      }
      let deletedFiles = [];
      files.forEach(file => {
        try {
          const fp = path.join(BACKUP_DIR, file);
          const st = fs.statSync(fp);
          if (st.mtime < cutoff) {
            fs.unlinkSync(fp);
            deletedFiles.push(file);
          }
        } catch (e) {
            console.log(e);
        }
      });
      if (deletedFiles.length > 0) {
        notifyAll({ event: 'backup', action: 'deleted', files: deletedFiles });
      }
      res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify({ message: 'устаревшие бэкапы были удалены', deleted: deletedFiles }));
    });
    return;
  }

  if (method === 'GET' && pathname === '/backup') {
    fs.readdir(BACKUP_DIR, (err, files) => {
      if (err) {
        res.writeHead(500, { 'Content-Type': 'application/json; charset=utf-8' });
        res.end(JSON.stringify({ error: 1, message: 'ошибка чтения директории backups' }));
        return;
      }
      res.writeHead(200, { 'Content-Type': 'application/json; charset=utf-8' });
      res.end(JSON.stringify(files));
    });
    return;
  }

  res.writeHead(404, { 'Content-Type': 'application/json; charset=utf-8' });
  res.end(JSON.stringify({ error: 4, message: 'Недопустимая точка запроса' }));
});

wss = new WebSocket.Server({ server });

wss.on('connection', ws => {
  console.log('подключен новый клиент');
  ws.send(JSON.stringify({ event: 'welcome', message: 'Subscribed to student updates' }));
  ws.on('close', () => {
    console.log('клиент отключился');
  });
});

try {
  fs.watch(BACKUP_DIR, (eventType, filename) => {
    if (!filename) return;
    notifyAll({ event: 'backup', action: 'fswatch', type: eventType, file: filename, time: new Date().toISOString() });
  });
} catch (e) {
  console.error('fs.watch error', e);
}

server.listen(PORT, () => {
  console.log(`server listening at http://localhost:${PORT}`);
});
