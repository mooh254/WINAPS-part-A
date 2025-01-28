//Registeration
import { initializeApp } from "https://www.gstatic.com/firebasejs/11.0.1/firebase-app.js";
import {
  getAuth,
  createUserWithEmailAndPassword,
} from "https://www.gstatic.com/firebasejs/11.0.1/firebase-auth.js";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyDWf2NV4dzwjvY9r2EFXyX5WIsznWEGk7g",
  authDomain: "mjismartair.firebaseapp.com",
  projectId: "mjismartair",
  storageBucket: "mjismartair.appspot.com",
  messagingSenderId: "375544400603",
  appId: "1:375544400603:web:5999f4619aedd94ed19afe"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const auth = getAuth(app);

//submit button
const signupButton = document.getElementById("signup-submitbutton");
signupButton.addEventListener("click", function (event) {
  event.preventDefault();

  //inputs
  const email = document.getElementById("email").value;
  const password = document.getElementById("password").value;

  createUserWithEmailAndPassword(auth, email, password)
    .then((userCredential) => {
      //Signed Up
      const user = userCredential.user;
      alert("Creating Account...");
      window.location.href = "userprofile.html";
      //...
    })
    .catch((error) => {
      const errorCode = error.code;
      const errorMessage = error.message;
      alert(errorMessage);
      //..
    });
});

