<template>
  <v-app>
    <router-view v-if="isAuthChecked" :key="$route.fullPath"></router-view>
  </v-app>
</template>

<script>
import { onAuthStateChanged } from "firebase/auth";
import { auth, messaging, onMessage } from "./firebase";
import { useRouter } from "vue-router";
import { ref, onMounted } from "vue";

export default {
  name: "App",
  setup() {
    const router = useRouter();
    const isAuthChecked = ref(false);

    onMessage(messaging, (payload) => {
      console.log("Nhận thông báo foreground:", payload);
      alert(payload.notification.body);
    });

    onMounted(() => {
      onAuthStateChanged(auth, (user) => {
        isAuthChecked.value = true;
        if (user) {
          router.push("/");
        }
      });
    });

    return { isAuthChecked };
  },
};
</script>
