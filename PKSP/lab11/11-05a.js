const rpcWSC = require("rpc-websockets").Client;

let ws = new rpcWSC("ws://localhost:4000/");

ws.on("open", () => {
  ws.call("square", [3]).then((answ) => {
    console.log(`square[3] = ${answ}`);
  });
  ws.call("square", [5, 4]).then((answ) => {
    console.log(`square[5, 4] = ${answ}`);
  });
  ws.call("sum", [2]).then((answ) => {
    console.log(`sum[2] = ${answ}`);
  });
  ws.call("sum", [2, 4, 6, 8, 10]).then((answ) => {
    console.log(`sum[2, 4, 6, 8, 10] = ${answ}`);
  });
  ws.call("mul", [3]).then((answ) => {
    console.log(`mul[3] = ${answ}`);
  });
  ws.call("mul", [3, 5, 7, 9, 11, 13]).then((answ) => {
    console.log(`mul[3, 5, 7, 9, 11, 13] = ${answ}`);
  });

  ws.login({ login: "admin", password: "admin" })
    .then(
      () => {
        ws.call("fib", [1])
          .catch((e) => {
            console.log("catch fib: ", e);
          })
          .then((answ) => {
            console.log(`fib[1] = ${answ}`);
          });
        ws.call("fib", [2])
          .catch((e) => {
            console.log("catch fib: ", e);
          })
          .then((answ) => {
            console.log(`fib[2] = ${answ}`);
          });
        ws.call("fib", [7])
          .catch((e) => {
            console.log("catch fib: ", e);
          })
          .then((answ) => {
            console.log(`fib[7] = ${answ}`);
          });
        ws.call("fact", [0])
          .catch((e) => {
            console.log("catch fact: ", e);
          })
          .then((answ) => {
            console.log(`fact[0] = ${answ}`);
          });
        ws.call("fact", [5])
          .catch((e) => {
            console.log("catch fact: ", e);
          })
          .then((answ) => {
            console.log(`fact[5] = ${answ}`);
          });
        ws.call("fact", [10])
          .catch((e) => {
            console.log("catchfact: ", e);
          })
          .then((answ) => {
            console.log(`fact[10] = ${answ}`);
          });
      },
      (error) => {
        console.error(error.message);
      }
    )
    .then(() => ws.close());
});
