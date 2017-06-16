from interop import Client

c = Client('htts://10.10.130.10:80', 'losangeles', '9886843481')

f = open('test.png', 'rb')
c.put_target_image(1, f)

