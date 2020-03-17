//보스가 하늘로 솟아 바위를 던지는 패턴 1의 스크립트입니다.
//폭발은 각각 바위에 지정된 스크립트를 통해 일어납니다.
//본 스크립트는 바위들을 생성만을 설정합니다.

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class throwingcontrol : MonoBehaviour {
    public float waittime = 1.0f;

    private float waittimer = 0.0f;
    private bool explode, isfalling = false;
    private Vector3 direction;
   // private Vector3 nowpos;
    private float fallingspd = 1.0f;
    private bool ison = false;
    private void OnCollisionEnter(Collision col)

    {

        if (col.gameObject.tag == "Ground")

            explode = true;
    }
    // Use this for initialization
    void Start()
    {
        bool ison = GameObject.Find("SpawnExplosion").GetComponent<spawnexplosion>().isspawn;

        //if (ison)
        //{
            Vector3 bosspos = GameObject.Find("Bip01").transform.position;
            Vector3 nowpos = GameObject.Find("SpawnExplosion").transform.position;

            direction = bosspos - nowpos;

            direction = -direction.normalized;

            //Debug.Log(nowpos);
            transform.position = bosspos;
            isfalling = true;
            fallingspd = GameObject.Find("SpawnExplosion").GetComponent<spawnexplosion>().fallingspd;
        //Debug.Log(bosspos);
        //}
    }

    // Update is called once per frame
    void Update()
    {
        //if (!ison)
        //    return;
        if (isfalling)
        {
            waittimer += Time.deltaTime;
            if(waittimer>waittime)
                transform.Translate(direction * fallingspd * Time.deltaTime, Space.World);
        }
        //if (explode)
        //    Destroy(gameObject);
    }

    void Setup(Vector3 bosspos, Vector3 targetpos, float spd)
    {
        direction = bosspos - targetpos;
        direction = direction.normalized;
        isfalling = true;

    }
}
