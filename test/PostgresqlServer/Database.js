// Database.js
const { Pool } = require('pg');
const logger = require('./Logger');
// Конфигурация подключения к базе данных
const pool = new Pool({
  user: 'TestUser',
  host: 'localhost',
  database: 'DatabaseAPITest',
  password: 'g7PAuarejhVzYtxXRG6bU2kK5dM8S3',
  port: 5432,
});

pool.on('error', (err) => {
  logger.error(err); 
});

// Функция для выполнения SQL-запроса
// const executeQuery = async (sql) => {
//   try {
//     const result = await pool.query(sql);
//     return result.rows;
//   } catch (err) {
//     throw err;
//   }
// };

module.exports = { pool };
