namespace ngl
{
    class ManageCsv<TAB> where TAB : ICsvRead, ICsv, new()
    {
        public static Dictionary<Int32, TAB> tablecsv = new Dictionary<Int32, TAB>();

        public static bool Load(string aname)
        {
            RCsv lcsv = new RCsv();
            if (lcsv.Read(aname) == false)
                return false;
            if (lcsv.ReadCsv(tablecsv) == false)
                return false;
            return true;
        }
    }
}