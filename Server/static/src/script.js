//sidebar
function showSidebar() {
  const sidebar = document.querySelector("#sidebar");
  sidebar.style.display = "flex";
}
function hideSidebar() {
  const sidebar = document.querySelector("#sidebar");
  sidebar.style.display = "none";
}

//copyright
const now = new Date();
const year = now.getFullYear();
const currentYear = document.querySelector(".year");
currentYear.innerHTML = `${year}`;

//form submit
function sendMessage() {
  var params = {
    name: document.getElementById("name").value,
    email: document.getElementById("email").value,
    contact: document.getElementById("phone").value,
    message: document.getElementById("message").value,
  };
  const serviceID = "report_service";
  const templateID = "contact_form";
  emailjs
    .send(serviceID, templateID, params)
    .then((message) => {
      console.log(message);
      Swal.fire({
        width: 180,
        icon: "success",
        iconColor: "#245501",
        title: "Message sent!",
        color: "#245501",
        showConfirmButton: false,
        timer: 1500,
        showClass: {
          popup: `
      animate__animated
      animate__fadeInDown
      animate__faster
    `,
        },
        hideClass: {
          popup: `
      animate__animated
      animate__fadeOutUp
      animate__faster
    `,
        },
      });
      return false;
    })
    .catch((err) => console.log(err));
}
function handleFormSubmit(e) {
  e.preventDefault();
  sendMessage();
}


