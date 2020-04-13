using System.Collections;
using System.Collections.Generic;
using System.IO;
using UnityEngine;
using UnityEngine.Networking;
public class owner_parser : MonoBehaviour {

    // Use this for initialization
    void Start () {
        Debug.Log("system init");
        StartCoroutine(WebRequest());

    }

    string url = "https://maplestory.nexon.com/Ranking/World/Total?c=%EB%A9%94%EC%A3%BC%ED%96%A5";
    string datapath = "C:/Users/seong/Desktop/projects_OSG/projects_OSG/personal/maple_check/";
    UnityWebRequest request;

    void WriteData(string strData)
    {
        FileStream f = new FileStream(datapath + "Data.txt", FileMode.Append, FileAccess.Write);
        StreamWriter writer = new StreamWriter(f, System.Text.Encoding.Unicode);
        writer.WriteLine(strData);
        writer.Close();
    }

    public void Parse()
    {
        StringReader sr = new StringReader(request.downloadHandler.text);
        string src = sr.ReadLine();
        string[] src_temp;

        //while (src != null) {
        int temp = src.IndexOf("rank_table");
        Debug.Log(temp);
        //}
    }
        IEnumerator WebRequest()
    {
        request = new UnityWebRequest();
        using (request = UnityWebRequest.Get(url))
        {
            yield return request.SendWebRequest();
            if (request.isNetworkError)
            {
                Debug.Log(request.error);
            }
            else
            {
                //Debug.Log(request.downloadHandler.text);
                var results = request.downloadHandler.text;
                WriteData(results);
                Debug.Log(results);
                Parse();
            }
        }
    }

    // Update is called once per frame
    void Update () {
		
	}
}
