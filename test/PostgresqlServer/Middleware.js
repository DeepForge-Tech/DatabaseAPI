// logMiddleware.js
const logger = require('./Logger');

const logMiddleware = (req, res, next) => {
  const start = Date.now();
  const originalSend = res.send;

  res.send = (data) => {
    const duration = Date.now() - start;
    const response = {
      status: res.statusCode,
      method: req.method,
      path: req.path,
      duration,
      data,
    };

    logger.info(response);
    originalSend.call(res, data);
  };

  next();
};

module.exports = logMiddleware;