const mysqlConnection = require("../database/db");
const util = require("util");
const query = util.promisify(mysqlConnection.query).bind(mysqlConnection);

const mqtt = require("mqtt");

const protocol = "ws";
const host = "broker.emqx.io";
const port = "8083";
const path = "/mqtt";
const clientId = `mqtt_${Math.random().toString(16).slice(3)}`;

const connectUrl = `${protocol}://${host}:${port}${path}`;

const client = mqtt.connect(connectUrl, {
  clientId,
  clean: true,
  connectTimeout: 4000,
  username: "",
  password: "",
  reconnectPeriod: 4000,
});

const TOPIC_TEMPERATURA = "topic/temperatura_G100";

/* ** ENDPOINT ** */
const publicacionesMQTT = async (req, res) => {
  const body = req.body;
  console.log("body", body);

  try {
    const resFechaHora = await query("SELECT NOW() as fechaFull;");

    const dataSensor = {
      topic: body.topic,
      valor: body.payload,
      fecha: resFechaHora[0].fechaFull,
    };

    await query("INSERT INTO sensores SET ?", dataSensor);

    res.status(200).json({ message: `Topic Recibido - ${body.topic}` });
  } catch (error) {
    console.error("ERROR: publicacionesMQTT - ", error);

    res.status(500).json({
      status: "FAILED",
      message: "Error al enviar solicitud de amistad",
    });
  }
};

/* ** EVENTOS MQTT ** */
const suscribirse = () => {
  client.subscribe([TOPIC_TEMPERATURA], () => {
    console.log(`Suscrito al topic '${TOPIC_TEMPERATURA}'`);
  });
};

const mensajesMQTT = async (topic, payload) => {
  console.log("Mensaje Recibido:", topic, payload.toString());

  try {
    const resFechaHora = await query("SELECT NOW() as fechaFull;");

    const dataSensor = {
      topic: topic,
      valor: payload.toString(),
      fecha: resFechaHora[0].fechaFull,
    };

    await query("INSERT INTO sensores SET ?", dataSensor);
  } catch (error) {
    console.error("ERROR: mensajesMQTT - ", error);
  }
};

module.exports = {
  client,
  suscribirse,
  mensajesMQTT,
  publicacionesMQTT,
};
