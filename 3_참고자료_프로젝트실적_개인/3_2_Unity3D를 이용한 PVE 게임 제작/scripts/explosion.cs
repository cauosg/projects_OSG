//보스가 공중에서 투하하는 돌이 땅에 닿으면 폭파시키는 스크립트입니다

using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class explosion : MonoBehaviour {
    public float explosionheight = 3.0f;
    public float explosiontime = 1.0f;
    public bool issample = false;
    public GameObject rock;
    public GameObject explode;
    
    private float counter = 0.0f;
    private bool exploding = false;
    // Use this for initialization
    void Start () {

        explode.SetActive(false);
        if (transform.position.y < -20)
            issample = true;
        else
            issample = false;

    }
	
	// Update is called once per frame
	void Update () {
        if (issample)
            return;

        if (rock.transform.position.y < explosionheight)
        {
            explode.SetActive(true);
            exploding = true;
            //Debug.Log("exploded");
        }
        if (exploding)
        {
            counter += Time.deltaTime;
            if (counter > explosiontime)
                Destroy(gameObject);
        }
    }

}
