extern crate websocket;
extern crate common;

pub mod websocket_extensions;

use std::sync::mpsc::{Sender as channel_sender};
use common::{LoginV1, Message, serialize_message, Type};
use self::websocket_extensions::{connect_to, send};

fn main() {
	let tx = connect_to("ws://127.0.0.1:2794".to_string());

	let test_login_msg: LoginV1 = LoginV1 { username: "Oipo".to_string(), password: "youbetcha".to_string() };
	let test_msg: Message = Message { msg_type: Type::Login, version: 1, content: serialize_message(&test_login_msg).unwrap() };
	let test_msg_str = serialize_message(&test_msg).unwrap();

	println!("json str: {:?}", test_msg_str);
	send(test_msg_str, tx);

	loop {

	}
}
