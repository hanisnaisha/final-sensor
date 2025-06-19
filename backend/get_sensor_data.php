<?php

header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json");

$servername = "localhost";  // or your DB server IP
$username = "humancmt_hn_smart_room_admin";
$password = "CCOt4iqctIKz";
$dbname = "humancmt_hn_smart_room";

$conn = new mysqli($servername, $username, $password, $dbname);
if ($conn->connect_error) {
  die(json_encode(["error" => "Connection failed: " . $conn->connect_error]));
}

$sql = "SELECT * FROM sensor_data ORDER BY timestamp DESC LIMIT 1";
$result = $conn->query($sql);

if ($result && $result->num_rows > 0) {
  echo json_encode($result->fetch_assoc());
} else {
  echo json_encode(["error" => "No data found"]);
}

$conn->close();
?>
