const express = require('express');
const cors = require('cors');
const admin = require('firebase-admin');
const serviceAccount = require('./admin/crud-test-e6735-firebase-adminsdk-3blu0-42071213ce.json');

const app = express();
const port = 3000;

admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: 'https://crud-test-e6735-default-rtdb.asia-southeast1.firebasedatabase.app/'
});
const db = admin.database();

// Middleware để parse JSON
app.use(express.json());
app.use(cors());

// Route để cập nhật giá trị vào trường code
app.put('/RFID/:code', async (req, res) => {
    const { code } = req.params; // Lấy code từ tham số URL
    const { value } = req.body; // Lấy giá trị từ body của yêu cầu

    try {
        // Cập nhật giá trị vào Firebase
        await db.ref(`RFID/${code}`).set(value);
        res.status(200).json({ message: `${code} updated successfully` });
    } catch (error) {
        res.status(500).json({ error: 'Error updating value: ' + error.message });
    }
});

// Lắng nghe cổng
app.listen(port, () => {
    console.log(`Server is running on http://localhost:${port}`);
});
