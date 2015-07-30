<?php
	
	/* 
	** WP-Search - Procura sites rodando wordpress em lista de domínios. 
	** Coded by Constantine - 07/2013.
	*/
	
	error_reporting(0);
	
	if ($_SERVER['argc'] != 3)
		die("\n Use: php {$_SERVER['argv'][0]} INDEX LINE\n".
			"  Ex: php {$_SERVER['argv'][0]} 1 host.net\n");
	
	$scan = new WPSearh();
	$scan->start();
	
	class WPSearh {
		private $index;
		private $content;
		
		public function __construct () {
			$this->index = $_SERVER['argv'][1];
			$this->content = $_SERVER['argv'][2];
		}
		
		public function start () {
			if ($this->exploit() === true) 
				$this->save();
		}
		
		private function exploit () {
			$header  = "GET /wp-login.php HTTP/1.1\r\n";
			$header .= "User-Agent: Mozilla/5.0 (Windows NT 6.1; Trident/7.0; rv:11.0) like Gecko\r\n";
			$header .= "Host: {$this->content}\r\n";
			$header .= "Connection: Close\r\n";
			$header .= "Accept-Encoding: identity\r\n\r\n";
			$IPv4 = gethostbyname($this->content);
			$result = false;
			
			if (($fp = fsockopen($IPv4, 80, $e, $err, 3)) !== false) {
				fputs($fp, $header);
				$response = '';
				while (!feof($fp)) {
					$response .= fread($fp, 100);
					if (strstr($response, "\r\n\r\n")) 
						break;
				}
				
				if (strstr($response, '200 OK')) {
					while (!feof($fp)) {
						$response .= fread($fp, 100);
						if (strstr($response, 'wp-submit')) {
							$result = true;
							break;
						}
					}
				}
				
				fclose($fp);
				if ($result === false)
					if (strstr($response, 'wp-submit'))
						$result = true;
			}
			
			return $result;
		}
		
		private function save () {
			if (($fp = fopen('results-wordpress.txt', 'a+')) !== false) {
				fwrite($fp, "http://{$this->content}/wp-login.php\n");
				fclose($fp);
			}
		}
	}
	
?>