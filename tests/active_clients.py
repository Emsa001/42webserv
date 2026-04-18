import requests
import threading
import time

def keepalive_request():
    with requests.Session() as s:
        # First request
        r1 = s.get("http://localhost:8080/")
        print(f"[{threading.current_thread().name}] First request status: {r1.status_code}")

        # Keep the connection open (simulate idle client)
        time.sleep(5)

        # Second request over the same connection
        r2 = s.get("http://localhost:8080/image.png")
        print(f"[{threading.current_thread().name}] Second request status: {r2.status_code}")

threads = []

# Simulate 10 concurrent clients
for i in range(1000):
    t = threading.Thread(target=keepalive_request, name=f"Client-{i+1}")
    threads.append(t)
    t.start()

for t in threads:
    t.join()
