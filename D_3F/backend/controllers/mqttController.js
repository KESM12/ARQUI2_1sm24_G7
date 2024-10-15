const pool = require("../database/db");
require("dotenv").config();

const mqtt = require("mqtt");

const protocol = process.env.MQTT_PROTOCOL || "ws";
const host = process.env.MQTT_HOST;
const port = process.env.MQTT_PORT;
const notifies = [
  {},
  {
    title: "Alerta de foco",
    message: "Ilumincación encedida sin presencia humanda.",
  },
  {
    title: "Alerta de foco",
    message: "Iluminación apagada para ahorro de energía.",
  },
  {
    title: "Alerta de calidad de aire",
    message: "Aire de la habitación en condiciones no optima.",
  },
  {
    title: "Alerta de calidad de aire",
    message: "Aire en la habitación en condiciones óptimas.",
  },
];
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
  pool.query("SELECT * from sensor_valores;", (error, result) => {
    console.log("Rows:", result.rows);
  });
  if (payload.toString().startsWith("eeprom")) {
  } else {
    var data = payload.toString().split(",");
    console.log("Data:", data);
    if (data.length > 7) {
      var proximity = data[0];
      var light = data[1];
      var airQuality = data[2];
      var temperature = data[3];
      var humidity = data[4];

      var notify = parseInt(data[8])
  
      var hora = null;
      var fecha = null;
      // Hacer consultas a la base de datos
 try {       


  
  // get hour now
  const hour = new Date().getHours();
  const minutes = new Date().getMinutes();
  const seconds = new Date().getSeconds();
  console.log("Hora:", hour, minutes);
  const hourminutes = `${hour}.${seconds}:00`;



      pool.query(
        `INSERT INTO sensor_valores (sensor_id, valor, hora) VALUES (1, ${temperature}, ${hourminutes});
        INSERT INTO sensor_valores (sensor_id, valor, hora) VALUES (2, ${light}, ${hourminutes});
        INSERT INTO sensor_valores (sensor_id, valor, hora) VALUES (3, ${airQuality}, ${hourminutes});
        INSERT INTO sensor_valores (sensor_id, valor, hora) VALUES (4, ${humidity}, ${hourminutes});
        INSERT INTO sensor_valores (sensor_id, valor, hora) VALUES (5, ${proximity}, ${hourminutes});`
      );

      if (notify > 0) {
        console.log("Notificación:", notify);
        console.log("Notificación:", notifies[notify]);
        pool.query(
          `INSERT INTO notificaciones (id, tipo, mensaje) VALUES (${notify},'${notifies[notify].title}', '${notifies[notify].message}');`
        );
      }
  } catch (error) {
    console.log("Error:", error);
  }}
  }
};

module.exports = {
  client,
  suscribirse,
  mensajesMQTT,
};
