const pool = require("../database/db");
require("dotenv").config();

const mqtt = require("mqtt");

const protocol = process.env.MQTT_PROTOCOL || "ws";
const host = process.env.MQTT_HOST;
const port = process.env.MQTT_PORT;

const path = "/mqtt";
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;

const connectUrl = `${protocol}://${host}:${port}${path}`;

const client = mqtt.connect(connectUrl, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  username: process.env.MQTT_USERNAME,
  password: process.env.MQTT_PASSWORD,
  reconnectPeriod: 4000,
});

const TOPIC = "project2g7";

/* ** EVENTOS MQTT ** */
const suscribirse = () => {
  client.subscribe([TOPIC], () => {
    console.log(`Suscrito al topic '${TOPIC}'`);
  });
};

const mensajesMQTT = async (topic, payload) => {
  console.log("Mensaje Recibido:", topic, payload.toString());

  if (message.toString().startsWith("eeprom")) {
  } else {
    var data = message.toString().split(",");
    if (data.length !== 9) return;

    var proximity = data[0];
    var light = data[1];
    var airQuality = data[2];
    var temperature = data[3];
    var humidity = data[4];

    var hora = null;
    var fecha = null;
    // Hacer consultas a la base de datos
    pool.query("SELECT NOW() as fechaFull;", (error, result) => {
      console.log("Error:", error);
      console.log("Result:", result);
      fecha = result.rows[0].fechafull;
    });

    pool.query(
      "SELECT ROUND((EXTRACT(HOUR FROM NOW()) + EXTRACT(MINUTE FROM NOW()) / 60.0 + EXTRACT(SECOND FROM NOW()) / 3600.0)::numeric, 4) AS hora_double;",
      (error, result) => {
        console.log("Error:", error);
        console.log("Result:", result);
        hora = result.rows[0].hora_double;
      }
    );

    pool.query(
      `INSERT INTO sensor_valores (sensor_id, valor, hora, fecha) VALUES (1, ${temperature}, ${hora}, ${fecha});
      INSERT INTO sensor_valores (sensor_id, valor, hora, fecha) VALUES (2, ${light}, ${hora}, ${fecha});
      INSERT INTO sensor_valores (sensor_id, valor, hora, fecha) VALUES (3, ${airQuality}, ${hora}, ${fecha});
      INSERT INTO sensor_valores (sensor_id, valor, hora, fecha) VALUES (4, ${humidity}, ${hora}, ${fecha});
      INSERT INTO sensor_valores (sensor_id, valor, hora, fecha) VALUES (5, ${proximity}, ${hora}, ${fecha});`
    );
  }
};

module.exports = {
  client,
  suscribirse,
  mensajesMQTT,
};
