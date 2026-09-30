import asyncio
from contextlib import asynccontextmanager

from fastapi import FastAPI, Request
from fastapi.staticfiles import StaticFiles
from fastapi.templating import Jinja2Templates
from pydantic import BaseModel


class SendData(BaseModel):
    message: str


tcp_writer: asyncio.StreamWriter | None = None
w5500_data = 0


async def tcp_server():
    async def handle_client(reader: asyncio.StreamReader, writer: asyncio.StreamWriter):
        global tcp_writer, w5500_data

        addr = writer.get_extra_info("peername")
        print(f"W5500 connected: {addr}")

        tcp_writer = writer

        try:
            while True:
                w5500_data = await reader.read(1024)
                w5500_data = w5500_data.decode()
                if not w5500_data:
                    break
                print("RX:", w5500_data)

        except Exception as e:
            print("TCP error:", e)

        finally:
            print("W5500 disconnected")
            if tcp_writer is writer:
                tcp_writer = None
            writer.close()
            await writer.wait_closed()

    server = await asyncio.start_server(handle_client, "0.0.0.0", 5000)
    print("TCP Server listening on port 5000")
    async with server:
        await server.serve_forever()


@asynccontextmanager
async def lifespan(app: FastAPI):
    task = asyncio.create_task(tcp_server())

    yield

    task.cancel()


app = FastAPI(lifespan=lifespan)
templates = Jinja2Templates(directory="templates")

app.mount("/static", StaticFiles(directory="static"), name="static")


@app.get("/")
async def home(request: Request):
    return templates.TemplateResponse(
        request,
        "index.html",
        context={"title": "W5500 控制中心", "status": "Connected"},
    )


@app.post("/send")
async def send(data: SendData):
    if tcp_writer is None:
        return {"success": False, "message": "W5500 not connected"}

    tcp_writer.write(data.message.encode())

    await tcp_writer.drain()

    return {"success": True, "message": data.message}


@app.get("/current")
async def current():
    if tcp_writer is None:
        return {"success": False, "message": "W5500 not connected"}

    return {"success": True, "message": w5500_data}
