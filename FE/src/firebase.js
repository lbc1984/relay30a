// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
import { getAuth } from "firebase/auth";

const firebaseConfig = {
  apiKey: "AIzaSyCXSu97M3_ONeEV5GL2bn9MheySzvjTAzQ",
  authDomain: "mqtt-d8e66.firebaseapp.com",
  projectId: "mqtt-d8e66",
  storageBucket: "mqtt-d8e66.firebasestorage.app",
  messagingSenderId: "324700447868",
  appId: "1:324700447868:web:9106ccd46a65a730ae95f9",
  measurementId: "G-R1EDFFDF0K"
};

const app = initializeApp(firebaseConfig);
const auth = getAuth(app);

export { auth };