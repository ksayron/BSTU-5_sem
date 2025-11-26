const http = require('http');

const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/socket',
  method: 'GET'
};

const req = http.request(options, (res) => {
  console.log('statusCode:', res.statusCode);
  console.log('statusMessage:', res.statusMessage || '(none)');
  console.log('remote address:', res.socket.remoteAddress);
  console.log('remote port   :', res.socket.remotePort);

  const chunks = [];
  res.on('data', c => chunks.push(c));
  res.on('end', () => {
    const body = Buffer.concat(chunks).toString();
    console.log('body:', body);
  });
});

req.on('error', (e) => console.error(e));
req.end();