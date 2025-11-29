const async = require("async");
const rpcClient = require("rpc-websockets").Client;

let ws = new rpcClient("ws://localhost:4000/");

ws.on("open", () => {
  ws.login({ login: "admin", password: "admin" }).then(() => {
    async.waterfall(
      [
        (cb) => {
          ws.call("square", [3])
            .then((r) => cb(null, r))
            .catch(cb);
        },

        (sq3, cb) => {
          ws.call("square", [5, 4])
            .then((r) => cb(null, sq3, r))
            .catch(cb);
        },

        (sq3, sq54, cb) => {
          ws.call("mul", [3, 5, 7, 9, 11, 13])
            .then((r) => cb(null, sq3, sq54, r))
            .catch(cb);
        },

        (sq3, sq54, mul357, cb) => {
          ws.call("sum", [sq3, sq54, mul357])
            .then((r) => cb(null, r))
            .catch(cb);
        },

        (sumResult, cb) => {
          ws.call("fib", [7])
            .then((r) => cb(null, sumResult, r))
            .catch(cb);
        },

        (sumResult, fibArr, cb) => {
          ws.call("mul", [2, 4, 6])
            .then((r) => cb(null, sumResult, fibArr, r))
            .catch(cb);
        },

        (sumResult, fibArr, mul246, cb) => {
          cb(null, sumResult + fibArr.slice(-1) * mul246);
        },
      ],

      (err, finalResult) => {
        if (err) {
          console.error("Error:", err);
        } else {
          console.log("Result:", finalResult);
        }
        ws.close();
      }
    );
  });
});
