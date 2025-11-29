const ws = require('ws');
const wss = new ws.Server({ port: 5001 }, () => console.log('Broadcast WS on ws://localhost:5001'));

wss.on('connection', ws => {
  ws.send('connected to broadcast server');

  ws.on('message', msg => {
    console.log('received:', msg.toString());
    wss.clients.forEach(client => {
      if (client.readyState === ws.OPEN) {
        client.send(`broadcast: ${msg.toString()}`);
      }
    });
  });

  ws.on('close', () => console.log('client disconnected'));
});
