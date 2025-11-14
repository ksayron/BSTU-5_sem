const send = require(`C:\\Users\\user\\AppData\\Roaming\\npm\\node_modules\\m0603-knp`)({
  email: "yourgmail@gmail.com",
  password: "api key",
  to: "yourgmail@gmail.com",
}).send;

send("<h1>Module testing global</h1>")
  .then((d) => console.log(d))
  .catch((e) => console.error(e));
