using System;
using System.IO;
using System.Runtime.InteropServices;

// This class just declares the functions that live inside RewindCore.dll,
// so the rest of our C# code can call them as if they were normal C# methods.
static class RewindCore
{
    // "IntPtr" is C#'s way of holding a raw memory address without knowing
    // what's at that address - exactly the "opaque handle" / "ticket" we
    // talked about. We never look inside it, we just pass it back and forth.

    [DllImport("RewindCore.dll")]
    public static extern IntPtr Rewind_Create();

    [DllImport("RewindCore.dll")]
    public static extern void Rewind_Destroy(IntPtr handle);

    // CharSet.Ansi tells .NET: "the strings I'm passing are plain narrow
    // text (char*), not the UTF-16 text C# uses internally" - it converts
    // automatically for us.
    [DllImport("RewindCore.dll", CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool Rewind_LoadCore(IntPtr handle, string corePath);

    [DllImport("RewindCore.dll", CharSet = CharSet.Ansi)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool Rewind_LoadGame(IntPtr handle, string gamePath);

    [DllImport("RewindCore.dll")]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool Rewind_RunFrame(IntPtr handle);

    [DllImport("RewindCore.dll")]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool Rewind_GetVideoFrame(
        IntPtr handle,
        out IntPtr data,
        out int width,
        out int height,
        out int pitch,
        out int pixelFormat);
}

class Program
{
    static void Main()
    {
        // Step 1: create a game session. "handle" is our ticket - we don't
        // know or care what it points to, we just hand it back every time.
        Console.WriteLine("Creating a game session...");
        IntPtr handle = RewindCore.Rewind_Create();

        // Step 2: load the core (the actual NES emulation engine).
        Console.WriteLine("Loading the NES core...");
        if (!RewindCore.Rewind_LoadCore(handle, "fceumm_libretro.dll"))
        {
            Console.WriteLine("FAILED to load core");
            return;
        }

        // Step 3: load the game itself.
        Console.WriteLine($"Current directory: {Environment.CurrentDirectory}");
        Console.WriteLine($"nestest.nes exists here? {File.Exists("nestest.nes")}");
        Console.WriteLine("Loading the game...");
        if (!RewindCore.Rewind_LoadGame(handle, "nestest.nes"))
        {
            Console.WriteLine("FAILED to load game");
            return;
        }

        // Step 4: run several frames so the game has time to actually draw
        // something (the very first frame or two are often blank).
        Console.WriteLine("Running 60 frames...");
        for (int i = 0; i < 60; i++)
        {
            RewindCore.Rewind_RunFrame(handle);
        }

        // Step 5: ask for the latest video frame.
        Console.WriteLine("Grabbing the video frame...");
        bool ok = RewindCore.Rewind_GetVideoFrame(
            handle, out IntPtr data, out int width, out int height, out int pitch, out int pixelFormat);

        if (!ok)
        {
            Console.WriteLine("FAILED to get a video frame");
            return;
        }

        Console.WriteLine($"Got a frame: {width}x{height}, pitch={pitch}, pixelFormat={pixelFormat}");

        // "data" is a raw address inside the C++ side's memory. Marshal.Copy
        // reads "size" bytes starting at that address and copies them into
        // a normal, safe C# byte array we can work with freely.
        int size = pitch * height;
        byte[] pixels = new byte[size];
        Marshal.Copy(data, pixels, 0, size);

        SaveAsBmp("output.bmp", pixels, width, height, pitch, pixelFormat);

        RewindCore.Rewind_Destroy(handle);
        Console.WriteLine("Done! Open output.bmp to see the frame.");
    }

    // This part has nothing to do with the C++ bridge anymore - it's just
    // "normal" C# work: writing a well-known file format (BMP) by hand,
    // byte by byte, so we don't need any extra image library for this test.
    static void SaveAsBmp(string path, byte[] pixels, int width, int height, int pitch, int pixelFormat)
    {
        if (pixelFormat != 1) // 1 = RETRO_PIXEL_FORMAT_XRGB8888
        {
            Console.WriteLine($"NOTE: pixel format {pixelFormat} isn't handled yet, the image may look wrong.");
        }

        int rowSizeOut = (width * 3 + 3) / 4 * 4; // BMP rows are padded to a multiple of 4 bytes
        int pixelDataSize = rowSizeOut * height;
        int fileSize = 14 + 40 + pixelDataSize;

        using var stream = new FileStream(path, FileMode.Create);
        using var writer = new BinaryWriter(stream);

        // --- BITMAPFILEHEADER (14 bytes) ---
        writer.Write((byte)'B'); writer.Write((byte)'M');
        writer.Write(fileSize);
        writer.Write(0); // reserved
        writer.Write(14 + 40); // offset to pixel data

        // --- BITMAPINFOHEADER (40 bytes) ---
        writer.Write(40);        // header size
        writer.Write(width);
        writer.Write(height);    // positive = rows stored bottom-to-top
        writer.Write((short)1);  // color planes
        writer.Write((short)24); // bits per pixel
        writer.Write(0);         // no compression
        writer.Write(pixelDataSize);
        writer.Write(0); writer.Write(0); // pixels per meter, not used
        writer.Write(0); writer.Write(0); // colors used / important, not used

        // --- Pixel data: BMP wants rows bottom-to-top, each pixel as B,G,R ---
        for (int y = height - 1; y >= 0; y--)
        {
            int rowStart = y * pitch;
            for (int x = 0; x < width; x++)
            {
                int i = rowStart + x * 4; // XRGB8888 = 4 bytes per pixel
                writer.Write(pixels[i + 0]); // B
                writer.Write(pixels[i + 1]); // G
                writer.Write(pixels[i + 2]); // R
            }
            for (int pad = width * 3; pad < rowSizeOut; pad++)
            {
                writer.Write((byte)0);
            }
        }
    }
}
