using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Mail;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace STNetUtils.SMTP
{
	public enum SMTPServerType
	{
		google,
		naver
	}

	public class SMTPSender
	{
		public static void SendSMTP(SMTPServerType smtpServerType, NetworkCredential SenderCredentials, string receiverEMail, string title, string message)
		{
			string smtpServer = $"smtp.{smtpServerType.ToString()}.com";
			try
			{
				using (MailMessage mail = new MailMessage())
				{
					mail.From = new MailAddress(SenderCredentials.UserName);
					mail.To.Add(receiverEMail);
					mail.Subject = title;
					mail.Body = message;
					mail.IsBodyHtml = false; // HTML 본문을 사용하려면 true

					using (SmtpClient smtp = new SmtpClient(smtpServer, 587))
					{
						smtp.Credentials = SenderCredentials;
						smtp.EnableSsl = true; // TLS 사용
						smtp.Send(mail);
						Console.WriteLine("이메일이 성공적으로 전송되었습니다!");
					}
				}
			}
			catch
			{
				Console.WriteLine("이메일이 전송되지않았음!");
			}
		}

	}
}

