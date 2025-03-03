<template>
  <div class="login-container">
    <button @click="loginWithGoogle">Đăng nhập</button>
  </div>
</template>

<style src="@/assets/css/login.css"></style>

<script>
import { auth } from "../firebase";
import { signInWithPopup, GoogleAuthProvider } from "firebase/auth";
import { useRouter } from "vue-router";

export default {
  setup() {
    const router = useRouter();

    const loginWithGoogle = async () => {
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
      }
    };

    return {loginWithGoogle };
  },
};
</script>
