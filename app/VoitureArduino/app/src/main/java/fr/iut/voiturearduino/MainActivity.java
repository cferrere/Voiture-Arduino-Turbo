package fr.iut.voiturearduino;

import androidx.appcompat.app.AppCompatActivity;

import android.net.InetAddresses;
import android.os.Bundle;
import android.os.StrictMode;
import android.util.Patterns;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.regex.Pattern;

public class MainActivity extends AppCompatActivity {

    private Button btnBackward;
    private Button btnForward;
    private Button btnLeft;
    private Button btnRight;
    private Button btnStop;

    private Button btnConnect;
    private TextView messageTv;
    private EditText inputHost;
    private EditText inputPort;

    private Socket client;
    private OutputStream output;
    private PrintWriter writer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        StrictMode.ThreadPolicy policy = new StrictMode.ThreadPolicy.Builder().permitAll().build();
        StrictMode.setThreadPolicy(policy);

        setContentView(R.layout.activity_main);

        btnBackward = (Button) findViewById(R.id.btnBackward);
        btnForward = (Button) findViewById(R.id.btnForward);
        btnLeft = (Button) findViewById(R.id.btnLeft);
        btnRight = (Button) findViewById(R.id.btnRight);
        btnStop = (Button) findViewById(R.id.btnStop);

        btnConnect = (Button) findViewById(R.id.btnConnect);
        messageTv = (TextView) findViewById(R.id.message);
        inputHost = (EditText) findViewById(R.id.inputHost);
        inputPort = (EditText) findViewById(R.id.inputPort);

        btnConnect.setOnClickListener(btn -> {
            connectSocket();
        });

        btnBackward.setOnClickListener(btn -> {
            sendMessage('b');
        });

        btnForward.setOnClickListener(btn -> {
            sendMessage('f');
        });

        btnStop.setOnClickListener(btn -> {
            sendMessage('s');
        });

        btnLeft.setOnClickListener(btn -> {
            sendMessage('l');
        });

        btnRight.setOnClickListener(btn -> {
            sendMessage('r');
        });
    }

    private void connectSocket() {
        try {
            if (client != null) {
                client.close();
                client = null;
                output = null;
                writer = null;
            }

            String host = inputHost.getText().toString();
            int port = Integer.parseInt(inputPort.getText().toString());

            if(host.trim().isEmpty()) {
                messageTv.setText(R.string.emptyHost);
            }

            if(port < 1024 || port > 65535) {
                messageTv.setText(R.string.invalidPort);
            }

            if(!host.matches(String.valueOf(Patterns.IP_ADDRESS))) {
                messageTv.setText(R.string.invalidHost);
            }

            client = new Socket(host, port);
            output = client.getOutputStream();
            writer = new PrintWriter(output, true);

            messageTv.setText(R.string.socketConnected);
        }catch(Exception e) {
            e.printStackTrace();
            messageTv.setText(R.string.errorSocket);
        }
    }

    private void sendMessage(char message) {
        if(output == null || writer == null) {
            messageTv.setText(R.string.socketNotConnected);
        }

        writer.println(message);
    }
}