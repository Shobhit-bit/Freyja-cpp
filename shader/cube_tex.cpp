class bitmap{
    public:
        Bitmap() = default;
        Bitmap(int w,int h,int comp,eBitmapFormat, fmt);
        Bitmap(int w,int h,int d,int comp,eBitmapFormat fmt);
        Bitmap(int w,int h,int comp,eBitmapFormat fmt,const void* ptr);
        int w_=0;
        int h_=0;
        int d_=1;
        int comp=3;
        eBitmapFormat fmt_ = eBitmapFormat_UnsignedByte;
        eBitmapType type_ = eBitmapType_2d;
        std::vector<uint8_t> data;
        static int getBytesPerComponents(eBitmapFormat fmt);
        void settPixel(int x,int y,const glm::vec4& c);
        glm::vec4 getPixel(int x,int y) const;
    }

