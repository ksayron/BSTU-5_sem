const send = require("m0603-knp")({
  email: "yourgmail@gmail.com",
  password: "api key",
  to: "yourgmail@gmail.com",
}).send;

send("<h1>Module testing local</h1>")
  .then((d) => console.log(d))
  .catch((e) => console.error(e));
