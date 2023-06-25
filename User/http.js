var xhr = new XMLHttpRequest();
xhr.open('GET', 'http://43.143.141.18:8080/api/v1/3U0fClbRjIwu5c5QxVkP/attributes', true);
xhr.onreadystatechange = function() {
  if (xhr.readyState === 4 && xhr.status === 200) {
    var response = JSON.parse(xhr.responseText);
    console.log(response);
  }
};
xhr.send();
