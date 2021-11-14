import org.json.JSONArray;
import org.json.JSONObject;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.PrintStream;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;

import java.awt.BorderLayout;
import java.awt.EventQueue;

import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.border.EmptyBorder;
import javax.swing.JTextField;
import javax.swing.JButton;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JLabel;
import java.awt.Font;
import java.util.ArrayList;

public class Finder extends JFrame {
	private static ArrayList times;
	public static double lamin, lomin, lamax, lomax;
	public static String queryName;

	private JPanel contentPane;
	private JTextField textField;
	public String userInput;
	private JButton btnExit;

	public static <frame> void main(String[] args) {

		lamin = 45.8389;
		lomin = 5.9962;
		lamax = 47.8229;
		lomax = 10.5226;
		queryName = "Orlando";

		String url = "https://opensky-network.org/api/states/all?lamin=" + lamin + "&lomin=" + lomin + "&lamax=" + lamax
				+ "&lomax=" + lomax;

		HttpClient client = HttpClient.newHttpClient();
		HttpClient client2 = HttpClient.newHttpClient();

		HttpRequest request = HttpRequest.newBuilder().uri(URI.create(url)).build();

		client.sendAsync(request, HttpResponse.BodyHandlers.ofString()).thenApply(HttpResponse::body)
				.thenAccept(Finder::parser).join();

	}

	public static void parser(String responseBody) {
		PrintStream ouputfile = null;
		try {
			ouputfile = new PrintStream(new File("report.txt"));
		} catch (Exception e) {
			System.out.println("Error creating output file");
		}

		String response = "[" + responseBody + "]";
		JSONArray planes = new JSONArray(response);
		times = new ArrayList();

		for (int i = 0; i < planes.length(); i++) {

			JSONObject plane = planes.getJSONObject(i);
			double time = (double) plane.getFloat("time");
			JSONArray states = plane.getJSONArray("states");

			for (int j = 0; j < states.length(); j++) {
				JSONArray statesArr = states.getJSONArray(j);
				String transponder = statesArr.getString(1);
				String origin = statesArr.getString(2);

				// System.out.println("callsign : " + transponder + " Origin: " + origin);
				// ouputfile.println("callsign : " + transponder + " Origin: " + origin);
				System.out.println("callsign : " + transponder + " Origin: " + origin);
				ouputfile.println(transponder);

			}
		}
	}

}
/*
 * public class frame extends JFrame {
 * 
 * private JPanel contentPane; private JTextField textField; public String
 * userInput; private JButton btnExit;
 * 
 * public static void main(String[] args) { EventQueue.invokeLater(new
 * Runnable() { public void run() { try { frame frame = new frame();
 * frame.setVisible(true); } catch (Exception e) { e.printStackTrace(); } } });
 * }
 * 
 * 
 * public frame() { setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
 * setBounds(100, 100, 499, 456); contentPane = new JPanel();
 * contentPane.setBorder(new EmptyBorder(5, 5, 5, 5));
 * setContentPane(contentPane); contentPane.setLayout(null);
 * 
 * textField = new JTextField(); textField.setBounds(116, 187, 244, 30);
 * contentPane.add(textField); textField.setColumns(10);
 * 
 * JButton btnNewButton = new JButton("Enter");
 * btnNewButton.addActionListener(new ActionListener() { public void
 * actionPerformed(ActionEvent e) { String name = textField.getText(); userInput
 * =name; } }); btnNewButton.setBounds(167, 253, 134, 52);
 * contentPane.add(btnNewButton);
 * 
 * JLabel lblNewLabel = new JLabel("Enter location Name");
 * lblNewLabel.setFont(new Font("Bernard MT Condensed", Font.PLAIN, 37));
 * lblNewLabel.setBounds(88, 68, 340, 94); contentPane.add(lblNewLabel);
 * 
 * btnExit = new JButton("Exit"); btnExit.addActionListener(new ActionListener()
 * { public void actionPerformed(ActionEvent e) { System.exit(0); } });
 * btnExit.setBounds(393, 386, 82, 23); contentPane.add(btnExit); } }
 * 
 * 
 */
