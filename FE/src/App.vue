<template>
  <v-app>
    <router-view v-if="isAuthChecked" :key="$route.fullPath"></router-view>
  </v-app>
</template>

<script>
import { onAuthStateChanged } from "firebase/auth";
import { auth } from "./firebase";
import { useRouter } from "vue-router";
import { ref, onMounted } from "vue";

export default {
  name: "App",
  setup() {
    const router = useRouter();
    const isAuthChecked = ref(false);

    onMounted(() => {
      onAuthStateChanged(auth, (user) => {
        isAuthChecked.value = true; // Đánh dấu đã kiểm tra Auth
        if (user) {
          router.push("/"); // Chuyển về trang chính nếu đã đăng nhập
        }
      });
    });

    return { isAuthChecked };
  },
};
</script>
