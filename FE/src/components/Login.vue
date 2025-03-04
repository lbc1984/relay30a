<template>
  <div class="login-container">
    <button @click="loginWithGoogle" :disabled="isLoading">
      <span v-if="isLoading" class="spinner"></span>
      <span v-else>Đăng nhập</span>
    </button>
  </div>
</template>

<style src="@/assets/css/login.css"></style>

<script>
import { auth } from "../firebase";
import { signInWithPopup, GoogleAuthProvider } from "firebase/auth";
import { useRouter } from "vue-router";
import { ref } from "vue";

export default {
  setup() {
    const router = useRouter();
    const isLoading = ref(false);

    const loginWithGoogle = async () => {
      isLoading.value = true;
      try {
        const provider = new GoogleAuthProvider();
        const result = await signInWithPopup(auth, provider);
        const user = result.user;
        const idToken = await user.getIdToken(true);

        localStorage.setItem("google_access_token", idToken);

        await new Promise((resolve) => setTimeout(resolve, 100));

        router.push("/");
      } catch (error) {
        console.error("Lỗi đăng nhập:", error.code, error.message);
        alert(`Lỗi: ${error.message}`);
      } finally {
        isLoading.value = false;
      }
    };

    return { loginWithGoogle, isLoading };
  },
};
</script>
