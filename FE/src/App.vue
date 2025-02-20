<template>
  <v-app>
    <h1>Device Management</h1>

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
            <div v-for="device in devices" :key="device.mac" class="device-card">
              <p><b>Name:</b> {{ device.name }}</p>
              <p><b>MAC:</b> {{ device.mac }}</p>
              <p><b>Status:</b> {{ device.status }}</p>
              <button @click="this.toggleDevice(device)">
                {{ device.status === 'off' ? 'Turn Off' : 'Turn On' }}
              </button>
            </div>
          </div>
        </v-col>
      </v-row>
    </v-container>
  </v-app>
</template>

<script>
import mqtt from 'mqtt';

export default {
  name: 'DeviceManager',
  data() {
    return {
      server_status: "offline",
      devices: [
      ],
      client: null,
    }
  },
  methods: {
    connectToMQTT(){
      this.client = mqtt.connect('wss://c812d6ed0a464712b9d2ce6524724c9e.s2.eu.hivemq.cloud:8884/mqtt', {
        username: 'lybaocuong',
        password: '1234@Abcd',
      });

      let _self = this

      this.client.on('connect', () => {
        console.log('Connected to MQTT');
        _self.server_status = "Connected with server"
        _self.client.subscribe('device/+/status');
        _self.client.subscribe('device/+/switch');
      });

      this.client.on('message', (topic, message) => {
        _self.handleMessage(topic, message.toString());
      });
    },

    handleMessage(topic, message){
      const macAddress = topic.split('/')[1];
      const status = message;

      console.log(topic, message)

      const index = this.devices.findIndex((d) => d.mac === macAddress);
      if (index >= 0) {
        this.devices[index].status = status;
      } else {
        this.devices.push({ mac: macAddress, status });
      }
    },

    toggleDevice(device) {
      device.switch == 'on' ? 'off' : 'on';
      console.log(`devices/${device.mac}/switch`, device.switch)
      this.client.publish(`devices/${device.mac}/switch`, device.switch);
    }
  },
  mounted(){
    this.connectToMQTT();    
  }
};
</script>

<style scoped>
.devices {
  display: flex;
}

.device-card {
  border: 1px solid #ccc;
  padding: 10px;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
  width: 250px;
  margin: 10px;
}

button {
  background-color: #4CAF50;
  color: white;
  padding: 5px 10px;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

button:hover {
  background-color: #45a049;
}
</style>
