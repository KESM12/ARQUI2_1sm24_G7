<script setup>
import ComponentStatus from "./components/ComponentStatus.vue";
import FormInput from "./components/FormInput.vue";
import { ref } from "vue";
import mqtt from "mqtt";

const showConnection = ref(false);

const values = ref({
  temperature: "0",
  light: "0",
  airQuality: "0",
  proximity: "0",
  fan: "",
});

const enable = ref({
  temperature: false,
  light: false,
  airQuality: false,
  proximity: false,
  fan: false,
});

const connection = ref({
  protocol: "ws",
  host: "broker.emqx.io",
  port: 8083,
  endpoint: "/mqtt",
  clean: true,
  connectTimeout: 30 * 1000,
  reconnectPeriod: 4000,
  clientId: "mqttx_661941b4",
  username: "",
  password: "",
});

const subscription = ref({
  topic: "testtopic/evtray",
  qos: 0,
});

const publish = ref({
  topic: "testtopic/evtray",
  qos: 0,
  payload: '{ "msg": "Hello, I am browser." }',
});

const receiveNews = ref("");

const qosList = [0, 1, 2];

const client = ref({
  connected: false,
});

const subscribeSuccess = ref(false);

const connecting = ref(false);

const retryTimes = ref(0);

const handleOnReConnect = () => {
  retryTimes.value += 1;
  if (retryTimes.value > 5) {
    try {
      client.value.end();
      retryTimes.value = 0;
      connecting.value = false;
    } catch (error) {
      console.log(error);
    }
  }
};

const createConnection = () => {
  connecting.value = true;
  const { protocol, host, port, endpoint, ...options } = connection.value;
  const connectUrl = `${protocol}://${host}:${port}${endpoint}`;
  client.value = mqtt.connect(connectUrl, options);
  if (client.value.on) {
    client.value.on("connect", () => {
      connecting.value = false;
      console.log("Connection succeeded!");
    });
    client.value.on("reconnect", handleOnReConnect);
    client.value.on("error", (error) => {
      console.log("Connection failed", error);
    });
    client.value.on("message", (topic, message) => {
      receiveNews.value = receiveNews.value.concat(message);
      console.log(`Received message ${message} from topic ${topic}`);
    });
  }
};

const doSubscribe = () => {
  const { topic, qos } = subscription.value;
  client.value.subscribe(topic, { qos }, (error, res) => {
    if (error) {
      console.log("Subscribe to topics error", error);
      return;
    }
    subscribeSuccess.value = true;
    console.log("Subscribe to topics res", res);
  });
};

const doUnSubscribe = () => {
  const { topic } = subscription.value;
  client.value.unsubscribe(topic, (error) => {
    if (error) {
      console.log("Unsubscribe error", error);
    }
  });
};

const doPublish = () => {
  const { topic, qos, payload } = publish.value;
  client.value.publish(topic, payload, { qos }, (error) => {
    if (error) {
      console.log("Publish error", error);
    }
  });
};

const destroyConnection = () => {
  if (client.value.connected) {
    try {
      client.value.end(false, () => {
        initData();
        console.log("Successfully disconnected!");
      });
    } catch (error) {
      console.log("Disconnect failed", error.toString());
    }
  }
};

const changeFanStatus = (status) => {
  enable.value.fan = status;
};
</script>

<template>
  <div class="">
    <div class="flex gap-4 p-8">
      <div>
        <font-awesome-icon
          class="w-10 h-10 text-white"
          :icon="['fab', 'dev']"
        />
      </div>
      <h1 class="text-2xl font-bold text-white text-center">
        Monitoreo de sensores
      </h1>
    </div>
    <div class="flex justify-end max-w-2xl m-auto">
      <button
        class="outline-none cursor-pointer"
        @click="showConnection = !showConnection"
      >
        <font-awesome-icon
          v-if="showConnection"
          class="text-white w-8"
          :icon="['fas', 'angle-down']"
        />
        <font-awesome-icon
          v-else
          class="text-white w-8"
          :icon="['fas', 'angle-up']"
        />
      </button>
    </div>
    <div v-if="showConnection" class="my-4 max-w-4xl m-auto">
      <div class="flex justify-center flex-wrap">
        <FormInput title="Host" v-model="connection.host" />
        <FormInput title="Port" v-model="connection.port" />
        <FormInput title="ClientId" v-model="connection.clientId" />
        <FormInput title="Username" v-model="connection.username" />
        <FormInput title="Password" v-model="connection.password" />
        <FormInput title="Topic" v-model="subscription.topic" />
        <FormInput title="publish" v-model="publish.topic" />
        <FormInput title="Payload" v-model="publish.payload" />
      </div>
      <div class="flex justify-center my-4">
        <button
          class="border border-white rounded-md text-white px-4 py-1"
          @click="createConnection"
        >
          Conectar
        </button>

        <button
          class="border border-white rounded-md text-white px-4 py-1 mx-4"
          @click="destroyConnection"
        >
          Desconectar
        </button>

        <button
          class="border border-white rounded-md text-white px-4 py-1"
          @click="doSubscribe"
        >
          Suscribir
        </button>

        <button
          class="border border-white rounded-md text-white px-4 py-1 ml-4"
          @click="doUnSubscribe"
        >
          Desuscribir
        </button>

        <button
          class="border border-white rounded-md text-white px-4 py-1 ml-4"
          @click="doPublish"
        >
          Publicar
        </button>
      </div>
    </div>

    <div class="p-4 max-w-4xl m-auto">
      <div>
        <div class="grid grid-cols-1 sm:grid-cols-2 gap-8 gap-y-8">
          <ComponentStatus
            title="Temperatura"
            :icon="['fas', 'fan']"
            :value="values.temperature"
            :enable="enable.temperature"
          />
          <ComponentStatus
            title="Luz en el ambiente"
            :icon="['fas', 'lightbulb']"
            :value="values.light"
            :enable="enable.light"
          />
          <ComponentStatus
            title="Calidad de aire"
            :icon="['fas', 'wind']"
            :value="values.airQuality"
            :enable="enable.airQuality"
          />
          <ComponentStatus
            title="Proximidad"
            :icon="['fas', 'people-arrows']"
            :value="values.proximity"
            :enable="enable.proximity"
          />
        </div>
        <div class="flex justify-center my-8">
          <div>
            <ComponentStatus
              title="Ventiladores"
              :icon="['fas', 'fan']"
              :value="values.fan"
              :enable="enable.fan"
            />
            <div class="flex justify-center">
              <button
                class="border border-white rounded-md text-white px-4 py-1"
                @click="changeFanStatus(!enable.fan)"
              >
                <font-awesome-icon
                  class="w-8 h-4"
                  :class="enable.fan ? 'text-green-300' : 'text-red-100'"
                  :icon="['fas', 'power-off']"
                />
              </button>
            </div>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<style scoped></style>
