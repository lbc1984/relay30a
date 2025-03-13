<template>
  <v-app>
    <router-view v-if="isAuthChecked" :key="$route.fullPath"></router-view>
  </v-app>
</template>

<script>
import { onAuthStateChanged } from "firebase/auth";
import { auth, messaging, getToken, onMessage } from "./firebase";
import { useRouter } from "vue-router";
import { ref, onMounted } from "vue";

export default {
  name: "App",
  setup() {
    const router = useRouter();
    const isAuthChecked = ref(false);
    const vapidKey =
      "BKqlJTulIqXQ3WHoHcKlnDjdSFklLylvKhCVa8rDSFwPF3UA2aNl2eFPChYWR0KLilnD2TiwZKVdckvRMvT146I"; // Thay bằng VAPID key của bạn

    const requestNotificationPermission = async () => {
      try {
        const token = await getToken(messaging, {
          vapidKey: vapidKey,
          serviceWorkerRegistration: await navigator.serviceWorker.register(
            "/firebase-messaging-sw.js"
          ),
        });
        console.log("FCM Token:", token);
      } catch (error) {
        console.error("Lỗi khi lấy token:", error);
      }
    };

    onMessage(messaging, (payload) => {
      console.log("Nhận thông báo foreground:", payload);
      alert(payload.notification.body);
    });

    onMounted(() => {
      onAuthStateChanged(auth, (user) => {
        isAuthChecked.value = true;
        if (user) {
          router.push("/");
          requestNotificationPermission();
        }
      });

      if ("serviceWorker" in navigator) {
        navigator.serviceWorker
          .register("/firebase-messaging-sw.js")
          .then((registration) => {
            console.log("Service Worker đăng ký thành công:", registration);
          })
          .catch((error) => {
            console.error("Service Worker đăng ký thất bại:", error);
          });
      }
    });

    return { isAuthChecked };
  },
};
</script>
