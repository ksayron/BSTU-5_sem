const send = require("./src/publish/m0603")({
  email: "yourgmail@gmail.com",
  password: "api key",
  to: "yourgmail@gmail.com",
}).send;

send("<h1>Module testing</h1>")
  .then((d) => console.log(d))
  .catch((e) => console.error(e));