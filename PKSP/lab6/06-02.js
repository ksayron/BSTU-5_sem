const http = require("http");
const fs = require("fs");
const querystring = require("querystring");
const nodemailer = require("nodemailer");

const EMAIL = "yourgmail@gmail.com";
const APP_PASSWORD = "api key";

const transporter = nodemailer.createTransport({
  service: "gmail",
  auth: {
    user: EMAIL,
    pass: APP_PASSWORD,
  },
  secure : true
});

http
  .createServer((req, res) => {
    if (req.method === "GET") {
      res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
      let html = fs.readFileSync("index.html");
      res.end(html);
    } else if (req.method === "POST") {
      let body = "";
      req.on("data", (chunk) => (body += chunk.toString()));
      req.on("end", async () => {
        const params = querystring.parse(body);

        const email = {
          from: params.from || EMAIL,
          to: params.to || EMAIL,
          html: `<div>${params.message || "нет текста"}</div>`,
        };

        try {
          const info = await transporter.sendMail(email);
          console.log(info, "\n===========================\n");
          res.writeHead(200, { "Content-Type": "text/html; charset=utf-8" });
          res.end(`<h2>Message sent!</h2><pre>${info.response}</pre>`);
        } catch (err) {
          res.writeHead(500, { "Content-Type": "text/html; charset=utf-8" });
          res.end(`<h2>Error occured:</h2><pre>${err}</pre>`);
        }
      });
    } else {
      res.writeHead(405, { "Content-Type": "text/plain; charset=utf-8" });
      res.end("Method not supported");
    }
  })
  .listen(5000, () => {
    console.log(`Server is running at: http://localhost:5000/`);
  });
