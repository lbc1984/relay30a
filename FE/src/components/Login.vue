<template>
    <div class="login-container">
      <button @click="loginWithGoogle">Đăng nhập</button>
    </div>
  </template>
  
  <style scoped>
  .login-container {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    height: 100vh; /* Chiều cao toàn màn hình */
    text-align: center;
  }
  
  button {
    padding: 10px 20px;
    font-size: 16px;
    cursor: pointer;
  }
  </style>
  

<script>
import { ref } from "vue";
import { auth } from "../firebase";
import {
  signInWithEmailAndPassword,
  signInWithPopup,
  GoogleAuthProvider,
  getAuth,
} from "firebase/auth";
import { useRouter } from "vue-router";

export default {
  setup() {
    const email = ref("");
    const password = ref("");
    const router = useRouter();

    const login = async () => {
      try {
        await signInWithEmailAndPassword(auth, email.value, password.value);
        alert("Đăng nhập thành công!");

        const auth = getAuth();
        auth.currentUser.getIdToken(true).then((idToken) => {
          alert(idToken);
        });
      } catch (error) {
        alert(error.message);
      }
    };

    const loginWithGoogle = async () => {
      try {
        const provider = new GoogleAuthProvider();
        const result = await signInWithPopup(auth, provider);

        const user = result.user;
        const idToken = await user.getIdToken(true);

        localStorage.setItem("google_access_token", idToken);
        router.push("/");
      } catch (error) {
        console.error("Lỗi đăng nhập:", error.code, error.message);
        alert(`Lỗi: ${error.message}`);
      }
    };

    return { email, password, login, loginWithGoogle };
  },
};
</script>
