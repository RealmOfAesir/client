extern crate websocket;
extern crate common;

use std::thread;
use std::sync::mpsc::channel;
use std::io::stdin;
use websocket::client::request::Url;
use websocket::{Client, Message, Sender, Receiver};
use websocket::message::Type;

fn main() {
	let url = "ws://127.0.0.1:2794".to_string();
	let ws_uri = Url::parse(&url[..]).unwrap();
	let request = Client::connect(ws_uri).unwrap();
	let response = request.send().unwrap();
	match response.validate() {
		Ok(()) => (),
		Err(e) => {
			println!("{:?}", e);
			return;
		}
	}
	let (mut sender, mut receiver) = response.begin().split();
	let (tx, rx) = channel();
	let tx_1 = tx.clone();
	println!("something something bullrap");

	let send_loop = thread::spawn(move || {
		loop {
			// Send loop
			let message: Message = match rx.recv() {
				Ok(m) => m,
				Err(e) => {
					println!("Send Loop: {:?}", e);
					return;
				}
			};
			match message.opcode {
				Type::Close => {
					println!("Received close");
					let _ = sender.send_message(&message);
					// If it's a close message, just send it and then return.
					return;
				},
				_ => (),
			}
			// Send the message
			match sender.send_message(&message) {
				Ok(()) => (),
				Err(e) => {
					println!("Send Loop: {:?}", e);
					let _ = sender.send_message(&Message::close());
					return;
				}
			}
		}
	});

	let receive_loop = thread::spawn(move || {
		// Receive loop
		for message in receiver.incoming_messages() {
			let message: Message = match message {
				Ok(m) => m,
				Err(e) => {
					println!("Receive Loop: {:?}", e);
					let _ = tx_1.send(Message::close());
					return;
				}
			};
			match message.opcode {
				Type::Close => {
					println!("Received close");
					// Got a close message, so send a close message and return
					let _ = tx_1.send(Message::close());
					return;
				}
				Type::Ping => match tx_1.send(Message::pong(message.payload)) {
					// Send a pong in response
					Ok(()) => (),
					Err(e) => {
						println!("Receive Loop: {:?}", e);
						return;
					}
				},
				// Say what we received
				_ => println!("Receive Loop: {:?}", message),
			}
		}
	});

	let test_login_msg: common::LoginV1 = common::LoginV1 { username: "Oipo".to_string(), password: "youbetcha".to_string() };
	let test_msg: common::Message = common::Message { msg_type: common::Type::Login, version: 1, content: common::serialize_message(&test_login_msg).unwrap() };
	let test_msg_str = match common::serialize_message(&test_msg).unwrap();
	println!("json str: {:?}", test_msg_str);
	tx.send(Message::text(test_msg_str));
	loop {

	}
}
