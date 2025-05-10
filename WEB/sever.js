const express = require('express');
const app = express();
const port = 3000;
const cors = require('cors');
const admin = require('firebase-admin');
const serviceAccount = require('./admin/crud-test-e6735-firebase-adminsdk-3blu0-42071213ce.json');
const WebSocket = require('ws');


admin.initializeApp({
    credential: admin.credential.cert(serviceAccount),
    databaseURL: 'https://crud-test-e6735-default-rtdb.asia-southeast1.firebasedatabase.app/'
  });
  const db = admin.database();
  const wss = new WebSocket.Server({ port: 8080 });
  const clients = new Map();


// Middleware để parse JSON
app.use(express.json());
app.use(cors());  



wss.on('connection', (ws) => {
  ws.once('message', (message) => {
      try {
          const data = JSON.parse(message);
          if (data.type === 'identify' && data.id) {
              console.log(`Client identified: ${data.id}`);
              ws.clientId = data.id; // Lưu ID của client
          }
      } catch (e) {
          console.error('Invalid identification message:', e.message);
      }
  });

  

  // Lắng nghe sự kiện 'message' từ ESP32-CAM
  ws.on('message', (data) => {
      if (data instanceof Buffer) {
          console.log('Received image data of size:', data.length);
          
          wss.clients.forEach((client) => {
              if (client.readyState === WebSocket.OPEN) {
                  client.send(data);
              }
          });
      }
  });

 
  ws.on('close', () => {
      console.log('Client disconnected');
  });
});

console.log("WebSocket server is running on ws://localhost:8080");

// có khách
app.get('/api/data/res', async (req, res) => {
  const ref = db.ref('POST/data'); // Đường dẫn tới dữ liệu
  try {
    const snapshot = await ref.get();
    const data = snapshot.val();
    
    if (data) {
      res.json(data);
    } else {
      res.status(404).json({ error: "No data found" });
    }
  } catch (error) {
    console.error("Error fetching data:", error);
    res.status(500).json({ error: error.message });
  }
});

app.get('/api/data/fire', async(req, res) => {
  const ref = db.ref('POST/chay')
  try{
    const snapshot = await ref.get()
    const data = snapshot.val();
    if(data){
      res.json(data)
    }
    else{
      res.status(404).json({error: "No data found"})
    }
  }catch(error){
    console.error("error fetching data:",error)
    res.status(500).json({error:error.message})
  }
})

// Route GET đơn giản để lấy dữ liệu từ Firebase
app.get('/api/data', (req, res) => {
  const ref = db.ref('sensor/dht11');
  ref.once('value', (snapshot) => {
    const data = snapshot.val();
    res.json(data);
  }).catch((error) => {
    res.status(500).json({ error: error.message });
  });
});


// Route POST để lưu dữ liệu vào Firebase
app.post('/api/data', (req, res) => {
  const ref = db.ref('LCD');
  ref.set(req.body)
    .then(() => res.json({ message: 'Đã gửi qua LCD' }))
    .catch(error => res.status(500).json({ error: error.message }));
});



app.put('/api/light', (req, res) => {
  const { status, onTime, offTime } = req.body; // Lấy status, onTime và offTime từ body của yêu cầu PUT
  const updates = {};

  // Kiểm tra nếu status hợp lệ (0 hoặc 1) và thêm vào đối tượng cập nhật
  if (typeof status === 'number' && (status === 0 || status === 1)) {
    updates.status = status;
  }

  // Kiểm tra nếu onTime hợp lệ và thêm vào đối tượng cập nhật
  if (typeof onTime === 'string' && onTime.match(/^\d{2}:\d{2}$/)) { // Định dạng HH:mm
    updates.onTime = onTime;
  }

  // Kiểm tra nếu offTime hợp lệ và thêm vào đối tượng cập nhật
  if (typeof offTime === 'string' && offTime.match(/^\d{2}:\d{2}$/)) { // Định dạng HH:mm
    updates.offTime = offTime;
  }

  // Kiểm tra nếu có ít nhất một trường để cập nhật
  if (Object.keys(updates).length === 0) {
    return res.status(400).json({ message: 'Invalid input. No valid fields to update.' });
  }

  // Cập nhật các trường cần thiết trong Firebase
  db.ref('/LIGHT/Light1').update(updates)
    .then(() => {
      res.status(200).json({ message: 'Hẹn giờ thành công', updates });
    })
    .catch((error) => {
      console.error('Error updating fields:', error);
      res.status(500).json({ message: 'Có lỗi xảy ra', error: error.message });
    });
})



app.put('/api/light/status1', (req, res) => {
  const { status1, onTime1, offTime1 } = req.body;
  const update = {};

  if (typeof status1 === 'number' && (status1 === 0 || status1 === 1)) {
    update.status1 = status1;
  }

  if (typeof onTime1 === 'string' && onTime1.match(/^\d{2}:\d{2}$/)) {
    update.onTime = onTime1;
  }

  if (typeof offTime1 === 'string' && offTime1.match(/^\d{2}:\d{2}$/)) {
    update.offTime = offTime1;
  }

  db.ref('/LIGHT/Light2').update(update)
    .then(() => {
      res.status(200).json({ message: 'Status1 updated successfully', update });
    })
    .catch((error) => {
      console.error('Error updating status1:', error);
      res.status(500).json({ message: 'Có lỗi xảy ra', error: error.message });
    });
});


app.get('/api/password', (req, res) => {
  const passwordRef = admin.database().ref('/settings/password'); // Đường dẫn đến trường mật khẩu
  passwordRef.once('value')
      .then((snapshot) => {
          const password = snapshot.val();
          if (password) {
              res.status(200).json({ password: password });
          } else {
              res.status(404).json({ message: 'Password not found' });
          }
      })
      .catch((error) => {
          console.error("Error fetching password:", error);
          res.status(500).json({ message: 'Failed to retrieve password', error: error.message });
      });
});


app.put('/RFID', async (req, res) => {
  const { value } = req.body; // Lấy giá trị từ body của yêu cầu

  if (!value) {
    return res.status(400).json({ error: 'No value provided' });
  }
  try {
    // Lấy tất cả các giá trị hiện tại trong Firebase
    const snapshot = await db.ref('/RFID').once('value');
    const existingData = snapshot.val() || {}; // Dữ liệu hiện tại hoặc rỗng nếu chưa có

    // Lặp qua các trường từ code1 đến code5
    for (let i = 1; i <= 5; i++) {
      const codeKey = `code${i}`; // Tạo tên trường, ví dụ 'code1', 'code2', ...
      
      if (!existingData[codeKey]) {
        // Nếu trường này trống, thì nhập giá trị
        await db.ref(`/RFID/${codeKey}`).set(value);
        return res.status(200).json({ message: `${codeKey} updated successfully` });
      }
    }

    // Nếu tất cả các trường đều đã có dữ liệu
    return res.status(400).json({ message: 'All codes are already filled' });
  } catch (error) {
    res.status(500).json({ error: 'Error updating value: ' + error.message });
  }
});


app.put('/api/door', (req, res) => {
  const newStatus = req.body.status; // Lấy status từ body của yêu cầu PUT

  // Kiểm tra nếu status không hợp lệ
  if (typeof newStatus !== 'number' || (newStatus !== 0 && newStatus !== 1)) {
    return res.status(400).json({ message: 'Invalid status value. Must be 0 or 1.' });
  }
  // Cập nhật giá trị của trường status
  db.ref('/POST/Door').update({ status: newStatus })
      .then(() => {
          res.status(200).json({ message: 'Status updated successfully', status: newStatus });
      })
      .catch((error) => {
          console.error('Error updating status:', error);
          res.status(500).json({ message: 'Failed to update status', error: error.message });
      });
});


app.get('/RFID', async (req, res) => {
  try {
    // Lấy toàn bộ dữ liệu từ đường dẫn /RFID
    const snapshot = await db.ref('/RFID').once('value');
    const data = snapshot.val();

    // Kiểm tra nếu không có dữ liệu
    if (!data) {
      return res.status(404).json({ message: 'No RFID data found' });
    }

    // Trả về dữ liệu dạng JSON
    res.status(200).json(data);
  } catch (error) {
    res.status(500).json({ error: 'Error fetching RFID data: ' + error.message });
  }
});


app.delete('/RFID/:code', async (req, res) => {
  const { code } = req.params; // Lấy mã RFID từ tham số URL
  
  try {
    // Kiểm tra xem mã RFID có tồn tại không
    const snapshot = await db.ref(`/RFID/${code}`).once('value');
    
    if (!snapshot.exists()) {
      return res.status(404).json({ message: `RFID code ${code} not found` });
    }

    // Xóa mã RFID
    await db.ref(`/RFID/${code}`).remove();
    
    res.status(200).json({ message: `RFID code ${code} deleted successfully` });
  } catch (error) {
    res.status(500).json({ error: 'Error deleting RFID code: ' + error.message });
  }
});


// API to get all users
app.get('/users', async (req, res) => {
  try {
    // Lấy danh sách tối đa 1000 tài khoản
    const listUsersResult = await admin.auth().listUsers(1000);

    // Lọc thông tin cần thiết từ mỗi user (không bao gồm mật khẩu)
    const users = listUsersResult.users.map(userRecord => {
      return {
        uid: userRecord.uid,
        email: userRecord.email,
        displayName: userRecord.displayName,
        emailVerified: userRecord.emailVerified,
        photoURL: userRecord.photoURL,
        disabled: userRecord.disabled
      };
    });

    // Trả về dữ liệu người dùng dưới dạng JSON
    res.status(200).json(users);
  } catch (error) {
    res.status(500).json({ error: 'Error fetching user data: ' + error.message });
  }
});

// API to delete user
app.delete('/users/:uid', async (req, res) => {
  const { uid } = req.params; // Lấy UID từ params
  try {
    // Xóa tài khoản người dùng trong Firebase Authentication
    await admin.auth().deleteUser(uid);

    res.status(200).json({ message: `User ${uid} deleted successfully` });
  } catch (error) {
    res.status(500).json({ error: 'Error deleting user: ' + error.message });
  }
});


// API để thêm tài khoản
app.post('/addUser', async (req, res) => {
  const { name, email, password } = req.body; 
  try {
    // Tạo người dùng mới trong Firebase Authentication
    const userRecord = await admin.auth().createUser({
      email: email,
      password: password,
      displayName: name
    });

    res.status(200).json({
      message: `Tài khoản ${userRecord.uid} đã được tạo thành công`,
      user: userRecord,
    });
  } catch (error) {
    res.status(500).json({ error: 'Lỗi khi tạo tài khoản: ' + error.message });
  }
});


function checkAndToggleLightStatus() {
  const currentTime = new Date().toLocaleTimeString('en-GB', { hour: '2-digit', minute: '2-digit' });

  // Lấy thông tin onTime, offTime, và status từ Firebase
  db.ref('/LIGHT/Light1').once('value')
    .then((snapshot) => {
      const { onTime, offTime, status } = snapshot.val();

      if (currentTime === onTime && status !== 1) {
        // Cập nhật trạng thái bật đèn trong Firebase
        db.ref('/LIGHT/Light1').update({ status: 1 })
          .then(() => console.log("Bật bóng đèn 1"));
      } else if (currentTime === offTime && status !== 0) {
        // Cập nhật trạng thái tắt đèn trong Firebase
        db.ref('/LIGHT/Light1').update({ status: 0 })
          .then(() => console.log("Tắt bóng đèn 1"));
      }
    })
    .catch((error) => {
      console.error('Lỗi khi lấy dữ liệu từ Firebase:', error);
    });
}


function checkAndToggleLightStatus2() {
  const currentTime = new Date().toLocaleTimeString('en-GB', { hour: '2-digit', minute: '2-digit' });
  // Lấy thông tin onTime, offTime, và status từ Firebase
  db.ref('/LIGHT/Light2').once('value')
    .then((snapshot) => {
      const { onTime, offTime, status1 } = snapshot.val();

      if (currentTime === onTime && status1 !== 1) {
        // Cập nhật trạng thái bật đèn trong Firebase
        db.ref('/LIGHT/Light2').update({ status1: 1 })
          .then(() => console.log("Bật bóng đèn 2"));
      } else if (currentTime === offTime && status1 !== 0) {
        // Cập nhật trạng thái tắt đèn trong Firebase
        db.ref('/LIGHT/Light2').update({ status1: 0 })
          .then(() => console.log("Tắt bóng đèn 2"));
      }
    })
    .catch((error) => {
      console.error('Lỗi khi lấy dữ liệu từ Firebase:', error);
    });
}


// Gọi hàm kiểm tra mỗi phút
setInterval(checkAndToggleLightStatus, 60000);
setInterval(checkAndToggleLightStatus2, 60000);




app.listen(port, () => {
    console.log(`Server running at http://localhost:${port}`);
  });






