extern crate websocket;

use std::thread;
use std::error;
use std::result;
use std::sync::mpsc::{channel, Sender as channel_sender};
use std::io::stdin;
use websocket::client::request::Url;
use websocket::{Client, Message, Sender, Receiver};
use websocket::message::Type;

pub fn connect_to<'a>(url: String) -> Option<channel_sender<websocket::Message<'a>>> {
	let ws_uri = Url::parse(&url[..]).unwrap();
	let request = Client::connect(ws_uri).unwrap();
	let response = request.send().unwrap();
	match response.validate() {
		Ok(()) => (),
		Err(e) => {
			println!("{:?}", e);
			return None;
		}
	}
	let (mut sender, mut receiver) = response.begin().split();
	let (tx, rx) = channel();
	let tx_in = tx.clone();

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
					let _ = tx_in.send(Message::close());
					return;
				}
			};
			match message.opcode {
				Type::Close => {
					println!("Received close");
					// Got a close message, so send a close message and return
					let _ = tx_in.send(Message::close());
					return;
				}

				Type::Ping => {
					match tx_in.send(Message::pong(message.payload)) {
						// Send a pong in response
						Ok(()) => (),
						Err(e) => {
							println!("Receive Loop: {:?}", e);
							return;
						}
					}
				},
				// Say what we received
				_ => println!("Receive Loop: {:?}", message),
			}
		}
	});

	Some(tx_in)
}

pub fn send(data: String, tx: channel_sender<websocket::Message>) {
	tx.send(Message::text(data));
}
