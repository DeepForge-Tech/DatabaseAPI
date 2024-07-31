const express = require("express");
const bodyParser = require("body-parser");
const bcrypt = require("bcrypt");
const { pool } = require("./Database");

const app = express();
const port = 8100;
const saltRounds = 10;
const plainTextPassword = "g7PAuarejhVzYtxXRG6bU2kK5dM8S3";

var hashPassword;
const hashing = async () => {
  await bcrypt.hash(plainTextPassword, saltRounds).then(async (hash) => {
    hashPassword = hash;
  });
};
hashing();

const user = "TestUser";

app.use(bodyParser.json());

app.post("/execute", async (req, res) => {
  //   const { username, password, sql } = req.body;
  const username = req.body.db_user;
  const password = req.body.db_password;
  const sql_query = req.body.query;

  if (!username || !password || !sql_query) {
    return res.status(400).json({
      error:
        "Пожалуйста, предоставьте все необходимые данные: username, password, sql.",
    });
  }

  //   const hashedPassword = users[username];
  //   console.log(hashedPassword);
  if (username == user) {
    const check = await bcrypt
      .compare(password, hashPassword)
      .then(async (state) => {
        if (state) {
          const result = await pool
            .query(sql_query)
            .then(async (output) => {
              res.json({ data: output.rows });
            })
            .catch((err) => {
              console.error(err);
              res.status(400).json({ message: err });
            });
        } else {
          res.status(401).json({ error: "Неверный пароль" });
        }
      })
      .catch((err) => {
        console.error(err);
        res.status(500).json({ error: "Ошибка сервера" });
      });
    // check();
  } else {
    res.status(401).json({ error: "Пользователь не найден" });
  }
});

app.listen(port, () => {
  console.log(`Server started on port ${port}`);
});
