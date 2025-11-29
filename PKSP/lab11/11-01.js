const ws = require("ws");
const fs = require("fs");

let k = 0;
let wss = new ws.Server({ port: 4000, host: "localhost" });

wss.on("connection", (ws) => {
  let duplex = ws.createWebSocketStream(ws, { encoding: "utf8" });
  let wfile = fs.createWriteStream(`./upload/file${++k}.txt`);
  duplex.pipe(wfile);

  wfile.on("finish", () => {
    console.log("Файл успешно получен и сохранен как " + `./upload/file${k}.txt`);
    ws.close();
  });
});

wss.on("error", (e) => {
  console.log(`error: ${e}`);
});
