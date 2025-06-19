<?php
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

header("Access-Control-Allow-Origin: *");
header("Content-Type: application/json");

// Ignore preflight OPTIONS requests
if ($_SERVER['REQUEST_METHOD'] === 'OPTIONS') {
    exit;
}

// Only allow POST
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    echo json_encode(['success' => false, 'error' => 'Invalid request method']);
    exit;
}

// Debug: log all requests
file_put_contents('relay_debug.txt', print_r($_SERVER, true) . print_r($_POST, true));

// 1. Get the relay value from POST
if (!isset($_POST['relay'])) {
    echo json_encode(['success' => false, 'error' => 'No relay value provided']);
    exit;
}

$relay = $_POST['relay'] === '1' ? 1 : 0;

// 2. Update relay state in your sensor_data table
$host = "localhost";
$user = "humancmt_hn_smart_room_admin";
$pass = "CCOt4iqctIKz";
$dbname = "humancmt_hn_smart_room";

$conn = new mysqli($host, $user, $pass, $dbname);
if ($conn->connect_error) {
    echo json_encode(['success' => false, 'error' => 'Database connection failed']);
    exit;
}

// Update the most recent row
$sql = "UPDATE sensor_data SET relay = $relay ORDER BY id DESC LIMIT 1";
if ($conn->query($sql) === TRUE) {
    echo json_encode(['success' => true]);
} else {
    echo json_encode(['success' => false, 'error' => 'Failed to update relay state: ' . $conn->error]);
}
$conn->close();
?>