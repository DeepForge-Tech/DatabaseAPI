const winston = require('winston');

const alignColorsAndTime = winston.format.combine(
	winston.format.colorize({
		all: true,
	}),
	winston.format.timestamp({
		format: 'YY-MM-DD HH:mm:ss',
	}),
	winston.format.printf((info) => `[${info.timestamp}]  ${info.level} : ${info.message}`)
);

const logger = winston.createLogger({
	level: 'info',
	format: winston.format.combine(
		winston.format.timestamp({
			format: 'YYYY-MM-DD HH:mm:ss.SSS',
		}),
		winston.format.printf((info) => {
			const timestamp = info.timestamp;
			const level = info.level.toUpperCase();
			const pid = process.pid;
			const message = info.message;

			return `[${timestamp}] ${level} (${pid}): ${message}`;
		})
	),
	transports: [
		new winston.transports.Console({
			format: winston.format.combine(winston.format.colorize(), alignColorsAndTime),
		}),
	],
});
module.exports = logger;