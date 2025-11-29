let WebSocket = require("ws");

let wss = new WebSocket.Server({ port: 5000, host: "localhost" });
let n = 0;

wss.on("connection", (ws) => {
  ws.on("pong", (data) => {
    console.log(`pong-server: ${data.toString()}`);
  });
  ws.on("message", (data) => {
    console.log("message: ", data.toString());
    ws.send(data);
  });

  setInterval(() => {
    wss.clients.forEach((client) => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(`11-03-server: ${++n}`);
      }
    });
  }, 15000);

  setInterval(() => {
    console.log();
    wss.clients.forEach((client) => {
      if (client.readyState === WebSocket.OPEN) {
        client.ping(`server: ping`);
      }
    });

    console.log(`available: ${wss.clients.size}`);
  }, 5000);
});
