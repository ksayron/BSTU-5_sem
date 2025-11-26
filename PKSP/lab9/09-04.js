// 09-04.js
const http = require('http');

const payload = {
  "__commment":"abc",
  "x":"1",
  "y":"2",
  "s":"message",
  "m":["a","b"],
  "o":{"surname":"AAA","name":"BBB"}
};

const data = JSON.stringify(payload);

const options = {
  hostname: 'localhost',
  port: 5000,
  path: '/json',
  method: 'POST',
  headers: {
    'Content-Type': 'application/json',
    'Content-Length': Buffer.byteLength(data)
  }
};

const req = http.request(options, res => {
  console.log('status:', res.statusCode);
  const bufs = [];
  res.on('data', c=>bufs.push(c));
  res.on('end', ()=> {
    const body = Buffer.concat(bufs).toString();
    console.log('body:', body);
    try {
      const obj = JSON.parse(body);
      console.log('parsed JSON response:', obj);
    } catch(e) { }
  });
});
req.on('error', console.error);
req.write(data);
req.end();