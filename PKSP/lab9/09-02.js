const http = require('http');
const qs = require('querystring');

const x=7, y=3;
const path = `/parameter?x=${x}&y=${y}`;


http.get({ hostname:'localhost', port:5000, path }, (res)=>{
  console.log('status:', res.statusCode);
  const bufs = [];
  res.on('data', c=>bufs.push(c));
  res.on('end', ()=> {
    console.log('body:', Buffer.concat(bufs).toString());
  });
}).on('error', console.error);
