<template>
  <v-app>
    <h1>Pubsub Management</h1>

    <v-container>
      <v-row>
        <v-col cols="6" md="6" sm="12">
          <v-table>
            <tbody>
              <tr>
                <td>Trạng thái</td>
                <td>{{ this.server_status }}</td>
              </tr>
            </tbody>
          </v-table>
        </v-col>
      </v-row>
      <v-row>
        <v-col cols="12">
          <div class="devices">
            <div
              v-for="device in devices"
              :key="device.mac"
              class="device-card"
            >
              <p>
                <b>Name:</b>
                <v-text-field
                  density="compact"
                  v-model="device.name"
                  @blur="saveName(device)"
                  @focus="name_old = device.name"
                ></v-text-field>
              </p>
              <p><b>MAC:</b> {{ device.mac }}</p>
              <p><b>Status:</b> {{ device.status }}</p>
              <button
                @click="this.toggleDevice(device)"
                :hidden="device.status === 'offline'"
              >
                {{ device.switch === "1" ? "Turn On" : "Turn Off" }}
              </button>
            </div>
          </div>
        </v-col>
      </v-row>
    </v-container>
  </v-app>
</template>

<script>
import mqtt from "mqtt";
import axios from "axios";

export default {
  name: "DeviceManager",
  data() {
    return {
      server_status: "offline",
      devices: [],
      client: null,
      name_old: "",
    };
  },
  methods: {
    connectToMQTT(url, username, password) {
      let port = 8884;
      let url_mqtt = "wss://" + url + ":" + port + "/mqtt";

      this.client = mqtt.connect(url_mqtt, {
        username: username,
        password: password,
      });

      let _self = this;

      this.client.on("connect", () => {
        console.log("Connected to MQTT");
        _self.server_status = "Connected with server";
        _self.client.subscribe("device/+/name");
        _self.client.subscribe("device/+/status");
        _self.client.subscribe("device/+/switch");
      });

      this.client.on("message", (topic, message) => {
        _self.handleMessage(topic, message.toString());
      });
    },

    handleMessage(topic, message) {
      const topic_split = topic.split("/");
      const macAddress = topic_split[1];
      const type = topic_split[2];

      console.log(topic, message);

      const item = this.devices.find((d) => d.mac === macAddress);
      if (item) {
        if (type == "status") item.status = message;
        else if (type == "switch") item.switch = message;
        else if (type == "name") item.name = message;
      } else {
        const device = {
          mac: macAddress,
          status: type == "status" ? message : "",
          switch: type == "switch" ? message : "",
          name: type == "name" ? message : "",
        };

        this.devices.push(device);
      }
    },

    toggleDevice(device) {
      let status_switch = device.switch == "1" ? "0" : "1";
      let item = this.devices.find((x) => x.mac == device.mac);
      item.switch = status_switch;

      this.client.publish(`device/${device.mac}/switch`, status_switch, {
        qos: 1,
        retain: true,
      });
    },
    saveName(device) {
      if (this.name_old != device.name)
        this.client.publish(`device/${device.mac}/name`, device.name, {
          qos: 1,
          retain: true,
        });
    },
  },
  mounted() {
    axios
      .get(
        "https://6yahrrwera.execute-api.ap-southeast-1.amazonaws.com/default/mqtt"
      )
      .then((response) => {
        console.log(response.data);

        let data = response.data;

        this.connectToMQTT(data.url, data.user, data.password);
      })
      .catch((error) => {
        console.error("Có lỗi xảy ra:", error);
      });
  },
};
</script>

<style src="@/assets/css/styles.css"></style>
