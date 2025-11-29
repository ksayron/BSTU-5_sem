let WebSocket = require("ws");

let ws = new WebSocket("ws://localhost:4000");

ws.on("open", () => {
  let duplex = WebSocket.createWebSocketStream(ws, { encoding: "utf8" });
  duplex.pipe(process.stdout);

  duplex.on("end", () => console.log());
});

ws.on("error", (e) => {
  console.log(`error: ${e}`);
});
