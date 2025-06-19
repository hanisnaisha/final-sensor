<?php
header("Content-Type: text/plain");

if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    http_response_code(405); // Method Not Allowed
    echo "Only POST method allowed.";
    exit;
}

// Validate required parameters
$required = ['temperature', 'humidity', 'flame', 'motion', 'relay'];
foreach ($required as $param) {
    if (!isset($_POST[$param])) {
        http_response_code(400); // Bad Request
        echo "Missing parameter: $param";
        exit;
    }
}

// Retrieve POST values
$temp = $_POST['temperature'];
$hum = $_POST['humidity'];
$flame = $_POST['flame'];
$motion = $_POST['motion'];
$relay = $_POST['relay'];

// MySQL credentials
$host = "localhost";        // or 127.0.0.1
$user = "humancmt_hn_smart_room_admin";
$pass = "CCOt4iqctIKz";
$dbname = "humancmt_hn_smart_room";

// Connect to database
$conn = new mysqli($host, $user, $pass, $dbname);

// Check connection
if ($conn->connect_error) {
    http_response_code(500);
    echo "Database connection failed.";
    exit;
}

// Insert data
$stmt = $conn->prepare("INSERT INTO sensor_data (temperature, humidity, flame, motion, relay) VALUES (?, ?, ?, ?, ?)");
$stmt->bind_param("diiii", $temp, $hum, $flame, $motion, $relay);

if ($stmt->execute()) {
    echo "OK";
} else {
    http_response_code(500);
    echo "Failed to insert data.";
}

$stmt->close();
$conn->close();
?>
