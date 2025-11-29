const WebSocket = require("ws");

const ws = new WebSocket("ws://localhost:5000");

ws.on("message", (msg) => {
  console.log("message: ", msg.toString());
});

ws.on("ping", (data) => {
  console.log(`ping-client: ${data.toString()}`);
});
